
-- ---------------------------------------------------------------------------------- --
-- ------------------ Procedure for Proces 5: Loaning an exemplar ------------------- --
-- ---------------------------------------------------------------------------------- --


CREATE OR REPLACE PROCEDURE create_loaned_exemplar(
    p_exemplar_name VARCHAR,
    p_exemplar_description VARCHAR,
    p_owner_id BIGINT,
    p_loaner_id BIGINT,
    p_category_id INT,
    p_loan_date TIMESTAMP,
    p_expected_receive_date TIMESTAMP,
    p_expected_return_date TIMESTAMP
)
AS $$
DECLARE
    v_exemplar_id BIGINT;
    v_row_count INT;
	v_existing_expo INT;
BEGIN
    
	-- Check if the loaner_id is the same as the owner_id
    IF p_loaner_id = p_owner_id THEN
        -- If loaner_id is the same as owner_id, raise a notice and return
        RAISE NOTICE 'Error: Loaner ID cannot be the same as Owner ID.';
        RETURN;
    END IF;
	
	-- Check if an exemplar with the given name and description already exists
    SELECT id INTO v_exemplar_id
    FROM exemplars
    WHERE name = p_exemplar_name AND description = p_exemplar_description;

    -- If exemplar does not exist, insert a new one
    IF v_exemplar_id IS NULL THEN
        -- Call insert_exemplar function to add the exemplar
        CALL insert_exemplar(
            p_exemplar_name,
            p_exemplar_description,
            p_owner_id,
            p_category_id,
			p_state := 'loaned'
        );
		
        -- Check if the exemplar was successfully inserted into the exemplars table
		SELECT id INTO v_exemplar_id
		FROM exemplars
		WHERE name = p_exemplar_name AND description = p_exemplar_description;

		-- If the exemplar was not inserted, raise a notice and return
		IF v_exemplar_id IS NULL THEN
			RAISE NOTICE 'Error: Failed to add exemplar to exemplars table.';
			RETURN;
		END IF;
		
		-- If the exemplar was inserted, raise a notice
		RAISE NOTICE 'Exemplar added to exemplars table';
	
	-- If exemplar does exist, check if it is being exhibited during the loan period
	ELSE
		-- Look through expositions associated with exemplar and see if one is currently running
        SELECT 1
        INTO v_existing_expo
        FROM expositions AS exp
        JOIN exposition_exemplar_zone AS eez ON exp.id = eez.exposition_id
        WHERE eez.exemplar_id = v_exemplar_id
        AND (
            (exp.real_start_date >= p_loan_date AND exp.real_start_date < p_expected_return_date)
            OR (exp.real_end_date > p_loan_date AND exp.real_end_date <= p_expected_return_date)
            OR (exp.real_start_date <= p_loan_date AND exp.real_end_date >= p_expected_return_date)
        );

        -- If the exemplar is being exhibited during the loan period, raise a notice and return
        IF FOUND THEN
            RAISE NOTICE 'Error: Exemplar is being exhibited during the loan period.';
            RETURN;
        END IF;

		-- If the exemplar is not being exhibited during the loan period, update its state to 'loaned'
		UPDATE exemplars
		SET state = 'loaned'
		WHERE id = v_exemplar_id;
    END IF;


    -- Insert into loaned_exemplars table
    INSERT INTO loaned_exemplars (exemplar_id, loan_date, expected_receive_date, loaner_id, expected_return_date)
    VALUES (v_exemplar_id, p_loan_date, p_expected_receive_date, p_loaner_id, p_expected_return_date);

    RAISE NOTICE 'Loan created successfully.';
END;
$$ LANGUAGE plpgsql;



-- ---------------------------------------------------------------------------------- --
-- ---------------------------------- Example calls --------------------------------- --
-- ---------------------------------------------------------------------------------- --

-- for existing exemplar
CALL create_loaned_exemplar(
    'Exemplar 3'::VARCHAR,   -- Exemplar name
    'Description 3'::VARCHAR, -- Exemplar description
    9::BIGINT, -- Owner ID
    1::BIGINT, -- Loaner ID
    1::INT, -- Category ID
    CURRENT_TIMESTAMP::TIMESTAMP, -- Loan date
    '2024-04-22 08:00:00'::TIMESTAMP, -- Expected receive date
    '2024-05-05 08:00:00'::TIMESTAMP  -- Expected return date
);


-- for non-existing exemplar
CALL create_loaned_exemplar(
    'Exemplar G'::VARCHAR,   -- Exemplar name
    'Description G'::VARCHAR, -- Exemplar description
    9::BIGINT, -- Owner ID
    1::BIGINT, -- Loaner ID
    1::INT, -- Category ID
    CURRENT_TIMESTAMP::TIMESTAMP, -- Loan date
    '2024-04-22 08:00:00'::TIMESTAMP, -- Expected receive date
    '2024-05-05 08:00:00'::TIMESTAMP  -- Expected return date
);







