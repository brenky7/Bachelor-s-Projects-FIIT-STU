
-- ---------------------------------------------------------------------------------- --
-- -------------- Procedure for Proces 4: Receiving a loaned exemplar --------------- --
-- ---------------------------------------------------------------------------------- --


CREATE OR REPLACE PROCEDURE receive_loaned_exemplar(p_id BIGINT, p_storage INT) AS $$
DECLARE
	v_exists BOOLEAN;
	v_exemplar_id BIGINT;
BEGIN
	-- Check if the loaned exemplar with the given ID exists
	SELECT EXISTS (SELECT 1 FROM loaned_exemplars WHERE id = p_id) INTO v_exists;
	
	-- If the loaned exemplar does not exist, raise a notice and return
	IF NOT v_exists THEN
		RAISE NOTICE 'Error: Loaned exemplar ID does not exist.';
		RETURN;
	END IF;
	
	-- Retrieve the exemplar ID associated with the loaned exemplar
    SELECT exemplar_id INTO v_exemplar_id FROM loaned_exemplars WHERE id = p_id;

    -- Update the info of the loaned exemplar in the loaned_exemplars table
    UPDATE loaned_exemplars 
	SET 
		state = 'delivered',
		receive_date = CURRENT_TIMESTAMP::TIMESTAMP
	WHERE id = p_id;

    -- Update the storage location of the exemplar in the exemplars table
    UPDATE exemplars SET storage_location = p_storage WHERE id = v_exemplar_id;

    -- Raise a notice to indicate successful update
    RAISE NOTICE 'Received loaned exemplar %, updated storage location to %', p_id, p_storage;
END;
$$ LANGUAGE plpgsql;



-- ---------------------------------------------------------------------------------- --
-- ---------------------------------- Example calls --------------------------------- --
-- ---------------------------------------------------------------------------------- --


CALL receive_loaned_exemplar(
    1, -- Loaned exemplar ID
    1    -- New storage location ID
);
