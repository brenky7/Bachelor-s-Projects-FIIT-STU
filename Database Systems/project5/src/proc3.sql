
-- ---------------------------------------------------------------------------------- --
-- ---------------- Procedure for Proces 3: Transfering an exemplar ----------------- --
-- ---------------------------------------------------------------------------------- --


CREATE OR REPLACE PROCEDURE transfer_exemplar_between_zones(
	p_exemplar_id BIGINT,
	p_old_zone_id BIGINT,
	p_new_zone_id BIGINT,
	p_exposition_id BIGINT
)
AS $$
DECLARE
	v_row_exists BOOLEAN;
	v_new_zone_exists BOOLEAN;
	v_other_exemplar_exists BOOLEAN;
BEGIN 
	-- Check if the provided association exists
	SELECT EXISTS (
		SELECT 1 
		FROM exposition_exemplar_zone eez
		WHERE (eez.exposition_id = p_exposition_id
			  	AND eez.zone_id = p_old_zone_id
			  	AND eez.exemplar_id = p_exemplar_id
			  )
	) 
	INTO v_row_exists;
	
	-- If not, raise notice and return
	IF NOT v_row_exists THEN
		RAISE NOTICE 'Error: Provided old association does not exist.';
		RETURN;
	END IF;
	
	SELECT EXISTS (
		SELECT 1
		FROM exposition_exemplar_zone eez
		WHERE (eez.exposition_id = p_exposition_id
			  	AND eez.zone_id = p_new_zone_id
			  )
	)
	INTO v_new_zone_exists;

	-- If the new zone is not associated with the exposition, raise notice and return
	IF NOT v_new_zone_exists THEN
		RAISE NOTICE 'Error: New zone is not associated with the exposition.';
		RETURN;
	END IF;

	-- Check if the old zone contains only the exemplar being transferred
	SELECT EXISTS (
		SELECT 1
		FROM exposition_exemplar_zone eez
		WHERE (eez.exposition_id = p_exposition_id
			  	AND eez.zone_id = p_old_zone_id
			  	AND eez.exemplar_id <> p_exemplar_id
			  )
	)
	INTO v_other_exemplar_exists;
	
	-- If there are other exemplars in the old zone, update the existing row to the new zone
	IF v_other_exemplar_exists THEN
		UPDATE exposition_exemplar_zone
		SET zone_id = p_new_zone_id
		WHERE (exposition_id = p_exposition_id
			AND exemplar_id = p_exemplar_id
			AND zone_id = p_old_zone_id
			);
		-- If there is a temporary row, delete it.
		DELETE FROM exposition_exemplar_zone
        WHERE exposition_id = p_exposition_id AND exemplar_id IS NULL AND zone_id = p_new_zone_id;	
		
	ELSE
		-- If the old zone contains other exemplars, insert a temporary row for the old zone
		INSERT INTO exposition_exemplar_zone (exposition_id, zone_id)
		VALUES (p_exposition_id, p_old_zone_id);
		RAISE NOTICE 'Temporary row created.';
		-- Update the existing row to the new zone
		UPDATE exposition_exemplar_zone
		SET zone_id = p_new_zone_id
		WHERE (exposition_id = p_exposition_id
			AND exemplar_id = p_exemplar_id
			AND zone_id = p_old_zone_id
			);
	END IF;
	
	RAISE NOTICE 'Exemplar transferred successfully.';
END;
$$ LANGUAGE plpgsql;



-- ---------------------------------------------------------------------------------- --
-- ---------------------------------- Example calls --------------------------------- --
-- ---------------------------------------------------------------------------------- --


CALL transfer_exemplar_between_zones(
    p_exemplar_id := 3, -- Sample ID of the exemplar to transfer
    p_old_zone_id := 3,   -- Sample ID of the old zone
    p_new_zone_id := 4,   -- Sample ID of the new zone
    p_exposition_id := 25 -- Sample ID of the exposition
);












